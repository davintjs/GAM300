using System;
using System.Collections;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BeanFactory;
using GlmSharp;

class GammaSettings : Script
{
    public GameObject gammaObj;

    // private sliders

    private float gammaFactor;

    void Start()
    {
        gammaFactor = Graphics.gamma;
    }

    void Update()
    {
        Graphics.gamma = gammaFactor;
    }
}
