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
        gammaSlider.InitializeSliderValue(Graphics.gamma);
    }

    void Update()
    {
        if(Graphics.gamma != gammaSlider.value)
            Graphics.gamma = gammaSlider.value;
    }
}
