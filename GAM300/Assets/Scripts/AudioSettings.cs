using System;
using System.Collections;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BeanFactory;
using GlmSharp;

class AudioSettings : Script
{
    public SliderButtonLogic masterSlider;
    public SliderButtonLogic sfxSlider;
    public SliderButtonLogic bgmSlider;

    void Start()
    {
        masterSlider.value = Audio.masterVolume;
        sfxSlider.value = Audio.sfxVolume;
        bgmSlider.value = Audio.musicVolume;

        masterSlider.UpdateSliderValue();
        sfxSlider.UpdateSliderValue();
        bgmSlider.UpdateSliderValue();
    }
    void Update()
    {
        Audio.masterVolume = Audio.masterVolume;
        Audio.sfxVolume = Audio.sfxVolume;
        Audio.musicVolume = Audio.musicVolume;
    }
}
