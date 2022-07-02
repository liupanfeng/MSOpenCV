package com.meishe.msopencv.activity;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;

import com.hjq.permissions.OnPermissionCallback;
import com.hjq.permissions.Permission;
import com.hjq.permissions.XXPermissions;
import com.meishe.msopencv.databinding.ActivityLaunchBinding;

import java.util.List;

public class LaunchActivity extends AppCompatActivity {

    static {
        System.loadLibrary("msopencv");
    }

    private ActivityLaunchBinding mBinding;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        mBinding = ActivityLaunchBinding.inflate(getLayoutInflater());
        setContentView(mBinding.getRoot());


        requestPermission();
        initListener();
    }

    private void initListener() {
        mBinding.btnIdCard.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                startActivity(new Intent(LaunchActivity.this
                        ,MSIDCardIdentificationActivity.class));
            }
        });

        mBinding.btnPic.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                startActivity(new Intent(LaunchActivity.this
                        ,PicReduceActivity.class));
            }
        });


        mBinding.btnCameraX.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                startActivity(new Intent(LaunchActivity.this
                        , CameraXPreViewActivity.class));
            }
        });
        mBinding.btnFace.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                startActivity(new Intent(LaunchActivity.this
                        ,CaptureActivity.class));
            }
        });
    }


    /**
     * 获取授权
     */
    private void requestPermission() {
        XXPermissions.with(this).permission(Permission.READ_EXTERNAL_STORAGE)
                .permission(Permission.WRITE_EXTERNAL_STORAGE)
                .permission(Permission.CAMERA)
                .permission(Permission.RECORD_AUDIO)
                .request(new OnPermissionCallback() {
                    @Override
                    public void onGranted(List<String> permissions, boolean all) {
                        if (all) {

                        } else {
                            finish();
                        }
                    }

                    @Override
                    public void onDenied(List<String> permissions, boolean never) {

                    }
                });
    }


}