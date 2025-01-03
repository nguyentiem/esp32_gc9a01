from PIL import Image
import matplotlib.pyplot as plt
def convert_to_header(image_path, output_path, array_name="image", width=240, height=240):
    # Mở ảnh PNG và chuyển sang RGB
    img = Image.open(image_path).convert('RGB')
    w, h = img.size
    # print(f"w= {w}, h={h}")
    # Tìm giá trị lớn nhất
    min_dimension = min(w, h)
    # Resize ảnh về kích thước phù hợp
    print(int(width*(w/min_dimension)))
    print(int(height*(h/min_dimension)))
    new_size=(int(width*(w/min_dimension)), int(height*(h/min_dimension)))
    img = img.resize(new_size, Image.Resampling.LANCZOS)
    w, h = img.size
    # Lấy dữ liệu pixel
    pixels = img.load()
    
    # Chuẩn bị dữ liệu mảng
    data = []
    for x in range(min(w, width)):
        for y in range(min(h, height)):
            r, g, b = pixels[x, y]
            # Chuyển đổi RGB888 sang RGB565
            # rgb565 = ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b)
            data.append(r)
            data.append(g)
            data.append(b)
    
    # Ghi dữ liệu vào file .h
    with open(output_path, 'w') as f:
        # Header thông tin
        f.write(f"#ifndef {array_name.upper()}_H\n")
        f.write(f"#define {array_name.upper()}_H\n\n")
        f.write("#include <stdint.h>\n")
        # f.write("#include <avr/pgmspace.h>\n\n")
        f.write(f"#define {array_name.upper()}_W {w}\n\n")
        f.write(f"#define {array_name.upper()}_H {h}\n\n")
        # Định nghĩa mảng const uint16_t
        f.write(f"const uint8_t {array_name}[]  = {{\n")
        
        # Ghi từng giá trị vào mảng (16 giá trị mỗi dòng)
        for i, value in enumerate(data):
            if i % 16 == 0:
                f.write("    ")
            f.write(f"0x{value:02X}, ")
            if (i + 1) % 16 == 0:
                f.write("\n")
        
        # Kết thúc mảng
        
        f.write("\n};\n\n")
        f.write(f"#endif // {array_name.upper()}_H\n")
    plt.imshow(img)
    plt.axis('off')  # Ẩn trục
    plt.show()
# Sử dụng hàm để tạo file .h
convert_to_header('at.jpg', 'output.h', array_name="my_image")