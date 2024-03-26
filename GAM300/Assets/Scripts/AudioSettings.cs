using System;
using System.Collections;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BeanFactory;
using GlmSharp;

class AudioSettings : Script
{
    public GameObject masterObj;
    public GameObject sfxObj;
    public GameObject bgmObj;

    // private sliders
    float masterVolume;
    float sfxVolume;
    float bgmVolume;

    void Start()
    {
        masterVolume = Audio.masterVolume;
        sfxVolume = Audio.sfxVolume;
        bgmVolume = Audio.musicVolume;
    }

    void Update()
    {
        Audio.masterVolume = masterVolume;
        Audio.sfxVolume = sfxVolume;
        Audio.musicVolume = bgmVolume;
    }
}
