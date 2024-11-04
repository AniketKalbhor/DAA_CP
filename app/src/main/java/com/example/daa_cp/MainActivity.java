package com.example.daa_cp;

import androidx.appcompat.app.AppCompatActivity;
import androidx.activity.result.ActivityResultLauncher;
import androidx.activity.result.contract.ActivityResultContracts;

import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;
import android.view.animation.AccelerateDecelerateInterpolator;

import com.example.daa_cp.databinding.ActivityMainBinding;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'daa_cp' library on application startup.
    static {
        System.loadLibrary("daa_cp");
    }

    private ActivityMainBinding binding;
    private TextView resultTextView;
    private Button selectFileButton;
    private Button compareButton;
    private TextView selectedFileNameTextView;
    private String fileContent;

    private ActivityResultLauncher<Intent> filePicker = registerForActivityResult(
            new ActivityResultContracts.StartActivityForResult(),
            result -> {
                if (result.getResultCode() == RESULT_OK && result.getData() != null) {
                    Uri uri = result.getData().getData();
                    readTextFromUri(uri);
                    showSelectedFileName(uri.getLastPathSegment());
                }
            }
    );

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        resultTextView = binding.resultTextView;
        selectFileButton = binding.selectFileButton;
        compareButton = binding.compareButton;
        selectedFileNameTextView = binding.selectedFileNameTextView;

        selectFileButton.setOnClickListener(v -> openFilePicker());
        compareButton.setOnClickListener(v -> compareCompression());
    }

    private void openFilePicker() {
        Intent intent = new Intent(Intent.ACTION_OPEN_DOCUMENT);
        intent.addCategory(Intent.CATEGORY_OPENABLE);
        intent.setType("text/*");
        filePicker.launch(intent);
    }

    private void readTextFromUri(Uri uri) {
        StringBuilder stringBuilder = new StringBuilder();
        try (InputStream inputStream = getContentResolver().openInputStream(uri);
             BufferedReader reader = new BufferedReader(new InputStreamReader(inputStream))) {
            String line;
            while ((line = reader.readLine()) != null) {
                stringBuilder.append(line);
            }
            fileContent = stringBuilder.toString();
            Toast.makeText(this, "File loaded successfully", Toast.LENGTH_SHORT).show();
        } catch (IOException e) {
            Toast.makeText(this, "Error reading file", Toast.LENGTH_SHORT).show();
        }
    }

    private void showSelectedFileName(String fileName) {
        selectedFileNameTextView.setText(fileName);
        selectedFileNameTextView.setAlpha(0f);
        selectedFileNameTextView.setVisibility(View.VISIBLE);
        selectedFileNameTextView.animate()
                .alpha(1f)
                .setDuration(300)
                .setInterpolator(new AccelerateDecelerateInterpolator())
                .start();
    }

    private void compareCompression() {
        if (fileContent == null || fileContent.isEmpty()) {
            Toast.makeText(this, "Please select a file first", Toast.LENGTH_SHORT).show();
            return;
        }

        // Call native methods
        String lzwResult = compressLZW(fileContent);
        String huffmanResult = compressHuffman(fileContent);
//        String deflateResult = compressDeflate(fileContent);
        int originalSize = fileContent.length();
        String result = String.format(
                "Original Size: %d bytes\n\nOutput LZW:\n%s\n\nOutput Huffman:\n%s",
                originalSize, lzwResult, huffmanResult
        );
        resultTextView.setText(result);
    }

    // Native method declarations
    public native String compressLZW(String input);
    public native String compressHuffman(String input);
    public native String compressDeflate(String input);
}