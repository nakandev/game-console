import math
import wave
import array


def test_spectrum_sampling():
    for n in range(1, 120):
        x = 27.5 * math.pow(2.0, float(n) / 12.0)
        print(n, x)
    # for n in range(0, 64):
    #     x = 27.5 * math.pow(2.0, float(n) / 6.0)
    #     print(n, x)


def test_wave():
    spectrums = [[0 for i in range(64)] for j in range(8)]
    for j in range(8):
        spectrums[j][20] = 48 - (j * 6)
        spectrums[j][25] = 48 - (j * 6)
        spectrums[j][30] = 32 - (j * 4)
        spectrums[j][32] = 32 - (j * 4)
        spectrums[j][35] = 32 - (j * 4)
        spectrums[j][37] = 16 - (j * 2)
        spectrums[j][40] = 16 - (j * 2)
        spectrums[j][42] = 16 - (j * 2)
        spectrums[j][45] = 16 - (j * 2)
    stream = []
    for i in range(32*1024):
        v = 0.0
        for fr in range(64):
            freq = 27.5 * math.pow(2.0, float(fr) / 6.0)
            t0 = i // (32 * 1024 // 8) * 2
            t0 = t0 if t0 < 8 else 7
            # print(i, fr, t0)
            amp = spectrums[t0][fr] / 16
            v += math.cos(i * freq / (32*1024) * 2 * math.pi) * amp
        v = int(v) + 128
        if not (0 <= v < 256):
            print(v)
        # v = int(math.cos(i * 440 / (32*1024) * 2 * math.pi) * 32) + 128
        stream.append(v)
    short_values = array.array('h', bytes(stream))
    wf = wave.open('test.wav', 'w')
    wf.setnchannels(1)
    wf.setsampwidth(1)  # 8-bits
    # wf.setsampwidth(2)  # 16-bits
    wf.setframerate(32*1024)
    wf.writeframes(short_values)
    wf.close()


test_spectrum_sampling()
# test_wave()
