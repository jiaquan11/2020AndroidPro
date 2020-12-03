package com.jiaquan.myplayer.util;

public class TimeUtil {
    /**
     * format times
     *
     * @param secds
     * @param totalsecds
     * @return
     */
    public static String secdsToDateFormat(int secds, int totalsecds) {
        long hours = secds / (60 * 60);//时
        long minutes = (secds % (60 * 60)) / (60);//分
        long seconds = secds % (60);//秒

        String sh = "00";
        if (hours > 0) {
            if (hours < 10) {
                sh = "0" + hours;
            } else {
                sh = hours + "";
            }
        }
        String sm = "00";
        if (minutes > 0) {
            if (minutes < 10) {
                sm = "0" + minutes;
            } else {
                sm = minutes + "";
            }
        }

        String ss = "00";
        if (seconds > 0) {
            if (seconds < 10) {
                ss = "0" + seconds;
            } else {
                ss = seconds + "";
            }
        }
        if (totalsecds >= 3600) {//需要展示小时
            return sh + ":" + sm + ":" + ss;
        }
        return sm + ":" + ss;

    }
}
