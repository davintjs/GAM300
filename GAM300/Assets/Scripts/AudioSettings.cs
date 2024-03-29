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
        masterSlider.InitializeSliderValue(Audio.masterVolume);
        sfxSlider.InitializeSliderValue(Audio.sfxVolume);
        bgmSlider.InitializeSliderValue(Audio.musicVolume);
    }
    void Update()
    {
        if(Audio.masterVolume != masterSlider.value)
            Audio.masterVolume = masterSlider.value;
        if (Audio.sfxVolume != sfxSlider.value)
            Audio.sfxVolume = sfxSlider.value;
        if (Audio.musicVolume != bgmSlider.value)
            Audio.musicVolume = bgmSlider.value;
    }
}
