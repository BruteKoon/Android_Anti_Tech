package com.example.client_protect;

import android.Manifest;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.os.Build;
import android.text.TextUtils;
import android.util.Log;

import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.SocketException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.NoSuchElementException;
import java.util.Scanner;

import static com.example.client_protect.Const.BINARY_SU;

public class Checker {

    private final Context mContext;
    private boolean loggingEnabled = true;

    boolean Root_Flag = false;

    public Checker(Context mContext) {
        this.mContext = mContext;
    }

    String test_flag(){
        return "RootChecker Flag";
    }

    boolean Get_Root_Flag(){
        return this.Root_Flag;
    }


    /**
     * A variation on the checking for SU, this attempts a 'which su'
     * @return true if su found
     */
    public boolean Check_Su_Exists(){
        Process process = null;
        try{
            process = Runtime.getRuntime().exec(new String[]{"which", BINARY_SU});
            BufferedReader in = new BufferedReader(new InputStreamReader(process.getInputStream()));
            return in.readLine() != null;
        } catch (IOException e) {
            e.printStackTrace();
        } finally{
            if (process != null) {
                process.destroy();
            }
        }
        return false;
    }

    /**
     * Using the PackageManager, check for a list of well known root apps. @link {Const.knownRootAppsPackages}
     * @param additionalRootManagementApps - array of additional packagenames to search for
     * @return true if one of the apps it's installed
     */

    public boolean Check_RootManagementApps(String[] additionalRootManagementApps) {

        // Create a list of package names to iterate over from constants any others provided
        ArrayList<String> packages = new ArrayList<>(Arrays.asList(Const.knownRootAppsPackages));
        if (additionalRootManagementApps!=null && additionalRootManagementApps.length>0){
            packages.addAll(Arrays.asList(additionalRootManagementApps));
        }

        return isAnyPackageFromListInstalled(packages);
    }

    /**
     * Using the PackageManager, check for a list of well known apps that require root. @link {Const.knownRootAppsPackages}
     * @param additionalDangerousApps - array of additional packagenames to search for
     * @return true if one of the apps it's installed
     */
    public boolean Check_PotentiallyDangerousApps(String[] additionalDangerousApps) {

        // Create a list of package names to iterate over from constants any others provided
        ArrayList<String> packages = new ArrayList<>();
        packages.addAll(Arrays.asList(Const.knownDangerousAppsPackages));
        if (additionalDangerousApps!=null && additionalDangerousApps.length>0){
            packages.addAll(Arrays.asList(additionalDangerousApps));
        }

        return isAnyPackageFromListInstalled(packages);
    }


    /**
     * Check if any package in the list is installed ( this package is root app )
     * @param packages - list of packages to search for
     * @return true if any of the packages are installed
     */
    private boolean isAnyPackageFromListInstalled(List<String> packages){
        boolean result = false;

        PackageManager pm = mContext.getPackageManager();
        for (String packageName : packages) {
            try {
                // Root app detected
                pm.getPackageInfo(packageName, 0);
                result = true;
            } catch (PackageManager.NameNotFoundException e) {
                // Exception thrown, package is not installed into the system
            }
        }
        return result;
    }



    /**
     *  check for this existence of "su" binary
     * @return true if "su" found
     */
    public boolean Check_For_Su_Binary(){
        String[] pathsArray = Const.getPaths();

        String filename = "su";

        boolean result = false;

        for (String path : pathsArray) {
            String completePath = path + filename;
            File f = new File(path, filename);
            boolean fileExists = f.exists();
            if (fileExists) {
                //binary detected!
                result = true;
            }
        }
        return result;
    }

    /**
     *  check for this existence of "busybox" binary
     * @return true if "su" found
     */
    public boolean Check_For_BusyBox(){
        String[] pathsArray = Const.getPaths();

        String filename = "busybox";

        boolean result = false;

        for (String path : pathsArray) {
            String completePath = path + filename;
            File f = new File(path, filename);
            boolean fileExists = f.exists();
            if (fileExists) {
                //binary detected!
                result = true;
            }
        }
        return result;
    }

    /**
     *  Exec "getprop" command
     * @return propety list
     */
    private String[] propsReader() {
        try {
            InputStream inputstream = Runtime.getRuntime().exec("getprop").getInputStream();
            if (inputstream == null) return null;
            String propVal = new Scanner(inputstream).useDelimiter("\\A").next();
            return propVal.split("\n");
        } catch (IOException | NoSuchElementException e) {
            return null;
        }
    }

    /**
     * Checks for several system properties for
     * @return - true if dangerous props are found
     */
    public boolean checkForDangerousProps() {

        final Map<String, String> dangerousProps = new HashMap<>();
        dangerousProps.put("ro.debuggable", "1");
        dangerousProps.put("ro.secure", "0");

        boolean result = false;

        String[] lines = propsReader();

        if (lines == null){
            // Could not read, assume false;
            return false;
        }

        for (String line : lines) {
            System.out.println("sukhoon " + line);
            for (String key : dangerousProps.keySet()) {
                if (line.contains(key)) {
                    String badValue = dangerousProps.get(key);
                    badValue = "[" + badValue + "]";
                    if (line.contains(badValue)) {
                        result = true;
                    }
                }
            }
        }
        return result;
    }

    /**
     * When you're root you can change the permissions on common system directories, this method checks if any of these patha Const.pathsThatShouldNotBeWritable are writable.
     * @return true if one of the dir is writable
     */
    public boolean checkForRWPaths() {

        boolean result = false;

        String[] lines = mountReader();

        if (lines == null){
            // Could not read, assume false;
            return false;
        }

        for (String line : lines) {

            // Split lines into parts
            String[] args = line.split(" ");

            if (args.length < 4){
                // If we don't have enough options per line, skip this and log an error
                // "Error formatting mount line
                continue;
            }

            String mountPoint = args[1];
            String mountOptions = args[3];

            for(String pathToCheck: Const.pathsThatShouldNotBeWritable) {
                if (mountPoint.equalsIgnoreCase(pathToCheck)) {

                    // Split options out and compare against "rw" to avoid false positives
                    for (String option : mountOptions.split(",")){

                        if (option.equalsIgnoreCase("rw")){
                            // path is mounted with rw permissions!
                            result = true;
                            break;
                        }
                    }
                }
            }
        }

        return result;
    }

    private String[] mountReader() {
        try {
            InputStream inputstream = Runtime.getRuntime().exec("mount").getInputStream();
            if (inputstream == null) return null;
            String propVal = new Scanner(inputstream).useDelimiter("\\A").next();
            return propVal.split("\n");
        } catch (IOException | NoSuchElementException e) {
            return null;
        }
    }


    //check the Google Installer ("com.android.vending" is google installer)
    public static boolean verifyGoogleInstaller(Context context){
        final String installer = context.getPackageManager().getInstallerPackageName(context.getPackageName());
        return installer != null && installer.startsWith("com.android.vending");
    }


    //chekc the Emulator (Using API) (true : emulator / false : no emulator)
    public boolean Check_For_Emulator(){
        return (Build.BRAND.startsWith("generic") && Build.DEVICE.startsWith("generic"))
                || Build.FINGERPRINT.startsWith("generic")
                || Build.FINGERPRINT.startsWith("unknown")
                || Build.HARDWARE.contains("goldfish")
                || Build.HARDWARE.contains("ranchu")
                || Build.MODEL.contains("google_sdk")
                || Build.MODEL.contains("Emulator")
                || Build.MODEL.contains("Android SDK built for x86")
                || Build.MANUFACTURER.contains("Genymotion")
                || Build.PRODUCT.contains("sdk_google")
                || Build.PRODUCT.contains("google_sdk")
                || Build.PRODUCT.contains("sdk")
                || Build.PRODUCT.contains("sdk_x86")
                || Build.PRODUCT.contains("vbox86p")
                || Build.PRODUCT.contains("emulator")
                || Build.PRODUCT.contains("simulator")
                || Build.BOARD.toLowerCase().contains("nox")
                || Build.BOOTLOADER.toLowerCase().contains("nox")
                || Build.HARDWARE.toLowerCase().contains("nox")
                || Build.PRODUCT.toLowerCase().contains("nox")
                || Build.SERIAL.toLowerCase().contains("nox");
    }


    public boolean Check_For_Emulator_Extend(){
        return true;
    }


    /*
     * Using the PackageManager, check for a list of well known apps that Speed Hacks. @link {Const.SpeedHacks}
     * @param additionalSpeedHacks - array of additional packagenames to search for
     * @return true if one of the apps it's installed
     */
    public boolean Check_For_Hack(String[] additionalSpeedHacks){
        // Create a list of package names to iterate over from constants any others provided
        ArrayList<String> packages = new ArrayList<>();
        packages.addAll(Arrays.asList(Const.SpeedHacks));
        if (additionalSpeedHacks!=null && additionalSpeedHacks.length>0){
            packages.addAll(Arrays.asList(additionalSpeedHacks));
        }
        return isAnyPackageFromListInstalled(packages);
    }


    public boolean Check_For_AttackTool(){
        return true;
    }




}