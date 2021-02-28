using UnityEngine;
using System.Runtime.InteropServices;
using UnityEngine.UI;


public class SonarCameraController : MonoBehaviour
{
    public RawImage cameraImage;
    public Transform markerPlaneTransform;
    private WebCamTexture webCamTexture;

    void Start()
    {
        webCamTexture = new WebCamTexture(WebCamTexture.devices[0].name, 640, 480);
        cameraImage.texture = webCamTexture;
        webCamTexture.Play();
        
        float focalLength = (float)(webCamTexture.width);
        Camera camera = GetComponent<Camera>();
        if (camera == null)
        {
            Debug.LogError("Camera component is found.");
        }
        else
        {
            float horizontalFieldOfView = Camera.VerticalToHorizontalFieldOfView(camera.fieldOfView, camera.aspect);
            focalLength = (float)(webCamTexture.width) / (2.0f * Mathf.Tan(horizontalFieldOfView * Mathf.Deg2Rad * 0.5f));
        }
        bool successInitialization = SonarLib.InitializeTrackingSystemForPinhole(1, webCamTexture.width, webCamTexture.height,
                                                                                 new Vector2(focalLength, focalLength), 
                                                                                 new Vector2(webCamTexture.width * 0.5f, webCamTexture.height * 0.5f));
        if (!successInitialization)
        {
            Debug.LogError("Sonar initialization fail.");
        }
        /*var cameraImageTransform = cameraImage.GetComponent<RectTransform>();
        if (webCamTexture.width > webCamTexture.height)
        {
            cameraImageTransform.localScale = new Vector3(1.0f, (float)(webCamTexture.width) / (float)(webCamTexture.height), 1.0f);
        }
        else
        {
            cameraImageTransform.localScale = new Vector3((float)(webCamTexture.height) / (float)(webCamTexture.width), 1.0f, 1.0f);
        }*/
    }

    void Update()
    {
        if (webCamTexture.isPlaying)
        {
            Color32[] colors = webCamTexture.GetPixels32();
            byte[] gray_colors = new byte[colors.Length];
            for (int i = 0; i < colors.Length; ++i)
            {
                gray_colors[i] = (byte)((colors[i].r + colors[i].g + colors[i].b) / 3);
            }
            TrackingState currentTrackingState = SonarLib.ProcessFrame(gray_colors, webCamTexture.width, webCamTexture.height);
            UpdatePose(currentTrackingState);
        }
    }

    private TrackingState lastTrackingsState = TrackingState.Undefining;
    private Quaternion lastQ;
    private Vector3 lastPosition;
    private float rotationSmooth = 0.9f;
    private float positionSmooth = 0.9f;

    void UpdatePose(TrackingState trackingState)
    {
        if (trackingState == TrackingState.Tracking)
        {
            Quaternion q;
            Vector3 position;
            SonarLib.GetCameraWorldPose(out q, out position);
            TransformFromMarkerPose(ref q, ref position);
            if (lastTrackingsState == TrackingState.Tracking)
            {
                q = Quaternion.Slerp(q, lastQ, rotationSmooth);
                position = Vector3.Slerp(position, lastPosition, positionSmooth);
                lastQ = q;
                lastPosition = position;
            }
            else
            {
                q = lastQ;
                position = lastPosition;
            }
            transform.localRotation = q;
            transform.localPosition = position;
        }
        lastTrackingsState = trackingState;
    }

    void TransformFromMarkerPose(ref Quaternion q, ref Vector3 position)
    {
        if (markerPlaneTransform == null)
            return;
        q = markerPlaneTransform.localRotation * q;
        position = markerPlaneTransform.localRotation * (position - markerPlaneTransform.localPosition);
    }
}

enum TrackingState: int
{
    Undefining = 0,
    Tracking,
    LostTracking
};

// Define the functions which can be called from the .dll.
internal static class SonarLib
{
    public static bool InitializeTrackingSystemForPinhole(int trackingSystemType, 
                                                          int imageWidth, int imageHeight,
                                                          Vector2 focalLength, Vector2 opticalCenter)
    {
        return sonar_initialize_tracking_system_for_pinhole(trackingSystemType, 
                                                            imageWidth, imageHeight, 
                                                            focalLength.x, focalLength.y, 
                                                            opticalCenter.x, opticalCenter.y);
    }

    public static TrackingState ProcessFrame(byte[] grayColors, int imageWidth, int imageHeight)
    {
        int trackingState = (int)TrackingState.Undefining;
        unsafe
        {
            fixed (byte* ptr = &grayColors[0])
            {
                trackingState = sonar_process_frame((System.IntPtr)ptr, imageWidth, imageHeight);
            }
        }
        return (TrackingState)trackingState;
    }

    public static Quaternion QuaternionFromMatrix(float[] m)
    {
        // Adapted from: http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/index.htm
        Quaternion q = new Quaternion();
        q.w = Mathf.Sqrt(Mathf.Max(0, 1 + m[0] + m[4] + m[8])) / 2;
        q.x = Mathf.Sqrt(Mathf.Max(0, 1 + m[0] - m[4] - m[8])) / 2;
        q.y = Mathf.Sqrt(Mathf.Max(0, 1 - m[0] + m[4] - m[8])) / 2;
        q.z = Mathf.Sqrt(Mathf.Max(0, 1 - m[0] - m[4] + m[8])) / 2;
        q.x *= Mathf.Sign(q.x * (m[7] - m[5]));
        q.y *= Mathf.Sign(q.y * (m[2] - m[6]));
        q.z *= Mathf.Sign(q.z * (m[3] - m[1]));
        return q;
    }

    public static bool GetCameraWorldPose(out Quaternion q, out Vector3 position)
    {
        float[] rotationMatrix_data = new float[9];
        float[] position_data = new float[3];
        bool successFlag = false;
        unsafe
        {
            fixed (float* r_ptr = &rotationMatrix_data[0])
            {
                fixed (float* p_ptr = &position_data[0])
                {
                    successFlag = SonarLib.sonar_get_camera_world_pose((System.IntPtr)r_ptr, (System.IntPtr)p_ptr);
                }
            }
        }
        q = QuaternionFromMatrix(rotationMatrix_data);
        position = new Vector3(position_data[0], position_data[1], position_data[2]);
        return successFlag;
    }

    [DllImport("sonar")]
    private static extern bool sonar_initialize_tracking_system_for_pinhole(int trackingSystemType,
                                                                            int imageWidth, int imageHeight,
                                                                            float fx, float fy, float cx, float cy);

    [DllImport("sonar")]
    private static extern int sonar_process_frame(System.IntPtr grayFrameData, int frameWidth, int frameHeight);

    [DllImport("sonar")]
    private static extern bool sonar_get_camera_local_pose(System.IntPtr localCameraRotationMatrixData, System.IntPtr localCameraTranslationData);

    [DllImport("sonar")]
    private static extern bool sonar_get_camera_world_pose(System.IntPtr worldCameraRotationMatrixData, System.IntPtr worldCameraPostionData);
}
