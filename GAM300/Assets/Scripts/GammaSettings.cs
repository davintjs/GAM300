using System;
using System.Collections;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BeanFactory;
using GlmSharp;

class GammaSettings : Script
{
    public SliderButtonLogic gammaSlider;

    void Start()
    {
        gammaSlider.value = Graphics.gamma;
        gammaSlider.UpdateSliderValue();
    }

    void Update()
    {
        Graphics.gamma = gammaSlider.value;
    }
}
