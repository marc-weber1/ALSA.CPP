#include <vector>
#include <cstring>

#include "ALSADevices.hpp"

#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"

float convertS16LEToFloat(const char sample[2]) {
    // Step 1: Combine the two bytes into a signed 16-bit integer
    int16_t intSample = (static_cast<int16_t>(static_cast<uint8_t>(sample[1])) << 8) |
                        static_cast<uint8_t>(sample[0]);

    // Step 2: Normalize to the range [-1.0, 1.0]
    return intSample / 32768.0f; // 32768 is 2^15, the maximum absolute value for int16_t
}

using namespace std;

int main(){
    ALSACaptureDevice microphone("plughw:0", 44100, 1, 1024, SND_PCM_FORMAT_S16_LE);
    vector<char> microphone_buffer(microphone.get_bytes_per_frame() * microphone.get_frames_per_period());
    vector<char> wav_samples(256 * 1024 * 4);

    microphone.open();

    for(int i=0; i<256; i++){
        microphone.capture_into_buffer(microphone_buffer.data(), 1024);

        for(int s=0; s < 1024; s++){
            float converted_sample = convertS16LEToFloat(microphone_buffer.data() + 2 * s);
            memcpy(wav_samples.data() + 1024 * 4 * i + 4 * s, &converted_sample, 4);
        }
    }

    microphone.close();

    drwav_data_format format;
    format.container = drwav_container_riff;     // <-- drwav_container_riff = normal WAV files, drwav_container_w64 = Sony Wave64.
    format.format = DR_WAVE_FORMAT_IEEE_FLOAT;          // <-- Any of the DR_WAVE_FORMAT_* codes.
    format.channels = 1;
    format.sampleRate = 44100;
    format.bitsPerSample = 32;

    drwav wav;
    drwav_init_file_write(&wav, "test.wav", &format, NULL);

    drwav_uint64 framesWritten = drwav_write_pcm_frames(&wav, 256 * 1024, wav_samples.data());
}