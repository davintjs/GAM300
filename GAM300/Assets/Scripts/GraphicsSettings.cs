using System;
using System.Collections;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BeanFactory;
using GlmSharp;

class GraphicsSettings : Script
{
    public GameObject fullscreenObj;
    public GameObject shadowObj;
    public GameObject bloomObj;

    private CheckBox fullscreenBtn;
    private CheckBox shadowBtn;
    private CheckBox bloomBtn;

    void Awake()
    {
        fullscreenBtn = fullscreenObj.GetComponent<CheckBox>();
        shadowBtn = shadowObj.GetComponent<CheckBox>();
        bloomBtn = bloomObj.GetComponent<CheckBox>();
    }

    void Start()
    {
        fullscreenBtn.Check(Graphics.fullscreen);
        shadowBtn.Check(Graphics.shadows);
        bloomBtn.Check(Graphics.bloom);
    }

    void Update()
    {
        Graphics.fullscreen = fullscreenBtn.IsChecked();
        Graphics.shadows = shadowBtn.IsChecked();
        Graphics.bloom = bloomBtn.IsChecked();
    }
}
