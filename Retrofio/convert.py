import os
import cv2

def convert_video_to_raw_bin(
    input_file="BadApple.mp4",
    output_file="bad_apple_raw.bin",
    width=32,
    height=24
):
    if not os.path.exists(input_file):
        print(f"Error: Could not find '{input_file}' in the current directory.")
        return

    cap = cv2.VideoCapture(input_file)
    if not cap.isOpened():
        print(f"Error: Failed to open '{input_file}'.")
        return

    total_frames = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))
    print(f"Processing '{input_file}' ({total_frames} frames) at {width}x{height}...")

    raw_bytes = bytearray()
    frame_count = 0

    while cap.isOpened():
        ret, frame = cap.read()
        if not ret:
            break

        # Convert frame to grayscale and resize to target matrix (32x24)
        gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
        resized = cv2.resize(gray, (width, height), interpolation=cv2.INTER_AREA)

        # Threshold to binary (0 = Black, 1 = White)
        _, bw = cv2.threshold(resized, 128, 1, cv2.THRESH_BINARY)

        # Pack 8 horizontal pixels into 1 raw byte (MSB first)
        flat_pixels = bw.flatten()
        for i in range(0, len(flat_pixels), 8):
            byte_val = 0
            for bit in range(8):
                if flat_pixels[i + bit]:
                    byte_val |= (1 << (7 - bit))
            raw_bytes.append(byte_val)

        frame_count += 1
        if frame_count % 300 == 0:
            print(f"Processed {frame_count}/{total_frames} frames...")

    cap.release()

    # Save output binary
    with open(output_file, "wb") as f:
        f.write(raw_bytes)

    print(f"Done! Created '{output_file}' ({len(raw_bytes)} bytes total).")

if __name__ == "__main__":
    convert_video_to_raw_bin()