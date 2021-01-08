package com.jiaquan.myplayer.util;

import android.media.MediaCodecList;

import java.util.HashMap;
import java.util.Map;

public class WLVideoSupportUtil {
    private static Map<String, String> codecMap = new HashMap<>();
    static {
        codecMap.put("h264", "video/avc");
    }

    public static String findVideoCodecName(String ffcodecname){
        if (codecMap.containsKey(ffcodecname)){
            return codecMap.get(ffcodecname);
        }
        return "";
    }

    public static boolean isSupportCodec(String ffcodecname){
        boolean supportVideo = false;
        int count = MediaCodecList.getCodecCount();
        for (int i = 0; i < count; i++){
            String[] types = MediaCodecList.getCodecInfoAt(i).getSupportedTypes();
            for (int j = 0; j < types.length; j++){
                if (types[j].equals(findVideoCodecName(ffcodecname))){
                    supportVideo = true;
                    break;
                }
            }
            if (supportVideo){
                break;
            }
        }
        return supportVideo;
    }
}
