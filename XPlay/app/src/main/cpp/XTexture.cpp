//
// Created by jiaqu on 2020/4/11.
//

#include "XTexture.h"
#include "XLog.h"
#include "XShader.h"
#include "XEGL.h"

class CXTexture : public XTexture{
public:
    XShader sh;
    XTextureType type;
    std::mutex mux;

    virtual void Drop(){
        mux.lock();
        XEGL::Get()->Close();
        sh.Close();
        mux.unlock();
        delete this;
    }

    virtual bool Init(void* win, XTextureType type){
        mux.lock();

        XEGL::Get()->Close();
        sh.Close();

        this->type = type;

        if (!win){
            mux.unlock();
            XLOGE("XTexture Init failed win is NULL");
            return false;
        }

        if (!XEGL::Get()->Init(win)) {
            mux.unlock();
            return false;
        }

        sh.Init((XShaderType)type);

        mux.unlock();
        return true;
    }

    virtual void Draw(unsigned char* data[], int width, int height){
        mux.lock();

#if 0
        static bool decflag = false;
        if (!decflag){
            FILE* fp = NULL;
            fp = fopen("/sdcard/test174.yuv", "wb");
            fwrite(data[0], width*height, 1, fp);
            fwrite(data[1], width*height/4, 1, fp);
            fwrite(data[2], width*height/4, 1, fp);
            fclose(fp);
            fp = NULL;

            decflag = true;
        }
#endif

        sh.GetTexture(0, width, height, data[0]);//Y
        if (type == XTEXTURE_YUV420P){
            sh.GetTexture(1, width/2, height/2, data[1]);//U
            sh.GetTexture(2, width/2, height/2, data[2]);//V
        }else{
            sh.GetTexture(1, width/2, height/2, data[1], true);//UV   此处不理解，后面再细察
        }

        sh.Draw();
        XEGL::Get()->Draw();

        mux.unlock();
    }
};

XTexture* XTexture::Create(){
    return new CXTexture();
}