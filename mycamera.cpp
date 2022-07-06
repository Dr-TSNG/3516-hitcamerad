#include <cstdlib>
#include <list>
#include <string>

#include "camera_kit.h"
#include "log.h"
#include "mycamera.h"
#include "udp.h"

using namespace OHOS;
using namespace OHOS::Media;

class MyFrameStateCallback : public FrameStateCallback {
    void OnFrameFinished(Camera& camera, FrameConfig& fc, FrameResult& result) override {
        LOGD("receive frame complete inform");
        if (fc.GetFrameConfigType() == FRAME_CONFIG_CAPTURE) {
            LOGD("capture frame received");
            std::list<Surface*> surfaceList = fc.GetSurfaces();
            for (Surface* surface: surfaceList) {
                SurfaceBuffer* buffer = surface->AcquireBuffer();
                if (buffer != nullptr) {
                    char* virtAddr = static_cast<char*>(buffer->GetVirAddr());
                    if (virtAddr != nullptr) {
                        if (SendUdp(virtAddr, buffer->GetSize())) {
                            LOGI("send udp successful");
                        } else {
                            LOGE("send udp failed");
                        }
                    }
                    surface->ReleaseBuffer(buffer);
                }
                delete surface;
            }
        }
        delete &fc;
    }
};

class CameraStateMng : public CameraStateCallback {
public:
    explicit CameraStateMng(EventHandler& eventHdlr) : eventHdlr_(eventHdlr) {}

    void OnCreated(Camera& c) override {
        LOGI("camera OnCreated");
        auto config = CameraConfig::CreateCameraConfig();
        config->SetFrameStateCallback(&fsCb_, &eventHdlr_);
        c.Configure(*config);
        cam_ = &c;
    }

    void OnCreateFailed(const std::string cameraId, int32_t errorCode) override {}

    void OnReleased(Camera& c) override {}

    void Capture() {
        if (cam_ == nullptr) {
            LOGE("camera is not ready");
            return;
        }
        auto* fc = new FrameConfig(FRAME_CONFIG_CAPTURE);
        Surface* surface = Surface::CreateSurface();
        if (surface == nullptr) {
            delete fc;
            LOGE("CreateSurface failed");
            return;
        }
        surface->SetWidthAndHeight(WIDTH, HEIGHT);
        fc->AddSurface(*surface);
        cam_->TriggerSingleCapture(*fc);
    }

    void Stop() {
        if (cam_ == nullptr) {
            LOGE("camera is not ready");
            return;
        }
        cam_->StopLoopingCapture();
    }

    MyFrameStateCallback fsCb_;

private:
    EventHandler& eventHdlr_;
    Camera* cam_ = nullptr;
};

namespace {
    EventHandler* mEventHandler;
    CameraStateMng* mCameraStateMng;
}

void InitCamera() {
    CameraKit* camKit = CameraKit::GetInstance();
    if (camKit == nullptr) {
        LOGE("failed to get CameraKit instance");
        exit(-1);
    }
    std::list<std::string> camList = camKit->GetCameraIds();
    std::string camId;
    for (auto& cam: camList) {
        LOGD("camera name: %s", cam.c_str());
        const CameraAbility* ability = camKit->GetCameraAbility(cam);
        /* find camera which fits user's ability */
        std::list<CameraPicSize> sizeList = ability->GetSupportedSizes(0);
        if (sizeList.size() != 0) {
            for (auto& pic: sizeList) {
                LOGD("Pic size: %d x %d", pic.width, pic.height);
                if (pic.width == WIDTH && pic.height == HEIGHT) {
                    camId = cam;
                    break;
                }
            }
        }
    }

    if (camId.empty()) {
        LOGE("no available camera");
        exit(-1);
    }

    mEventHandler = new EventHandler();
    mCameraStateMng = new CameraStateMng(*mEventHandler);
    camKit->CreateCamera(camId, *mCameraStateMng, *mEventHandler);
}

void Capture() {
    mCameraStateMng->Capture();
}
