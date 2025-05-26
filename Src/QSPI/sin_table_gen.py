import numpy as np

# Параметры сигнала
f = 50             # Частота синуса (Гц)
Fs = 1000000       # Частота дискретизации (Гц)
N = int(Fs / f)    # Число точек
dac_bits = 12      # Разрядность ЦАП
amplitude = (2**dac_bits // 2) - 1  # 2047 для 12 бит

# Генерация синуса
t = np.linspace(0, 1/f, N, endpoint=False)
sine_values = amplitude * np.sin(2 * np.pi * f * t) + (2**dac_bits // 2)

# Преобразование в целые числа и ограничение диапазона
sine_values = np.clip(sine_values, 0, 2**dac_bits - 1).astype(int)

# Создание .h файла
with open("sine_table.h", "w") as f:
    f.write("#ifndef SINE_TABLE_H\n")
    f.write("#define SINE_TABLE_H\n\n")
    f.write("#include <stdint.h>\n\n")
    f.write("#define QSPI_MEM __attribute__((section(\".extflash_text\"), aligned(4)))\n\n")
    f.write("extern const uint16_t sine_table[];\n\n")
    f.write("#endif // SINE_TABLE_H\n")

# Создание .c файла
with open("sine_table.c", "w") as f:
    f.write("#include \"sine_table.h\"\n\n")
    f.write("QSPI_MEM const uint16_t sine_table[{}] = {{\n".format(N))
    for i, val in enumerate(sine_values):
        f.write(f"    {val}," + ("\n" if (i+1) % 10 == 0 else " "))
    f.write("\n};\n")