using System;
using System.Collections;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using BeanFactory;
using GlmSharp;

public class Settings : Script
{
    public GameObject audioButtonObj;
    public GameObject graphicsButtonObj;
    public GameObject gammaButtonObj;

    public GameObject audioCanvas;
    public GameObject graphicsCanvas;
    public GameObject gammaCanvas;

    SpriteRenderer audioButton;
    SpriteRenderer graphicsButton;
    SpriteRenderer gammaButton;

    AudioSettings audioSettings;
    GraphicsSettings graphicsSettings;
    GammaSettings gammaSettings;

    public int currentMenu = 0;

    void Awake()
    {
        audioButton = audioButtonObj.GetComponent<SpriteRenderer>();
        graphicsButton = graphicsButtonObj.GetComponent<SpriteRenderer>();
        gammaButton = gammaButtonObj.GetComponent<SpriteRenderer>();

        audioSettings = audioCanvas.GetComponent<AudioSettings>();
        graphicsSettings = graphicsCanvas.GetComponent<GraphicsSettings>();
        gammaSettings = gammaCanvas.GetComponent<GammaSettings>();
    }

    void Start()
    {
        audioCanvas.SetActive(false);
        graphicsCanvas.SetActive(false);
        gammaCanvas.SetActive(false);
    }

    void Update()
    {
        if(audioButton != null && audioButton.IsButtonClicked())
        {
            SelectMenu(0);
        }

        if (graphicsButton != null && graphicsButton.IsButtonClicked())
        {
            SelectMenu(1);
        }

        if (gammaButton != null && gammaButton.IsButtonClicked())
        {
            SelectMenu(2);
        }
    }

    void SelectMenu(int menu)
    {
        currentMenu = menu;

        audioCanvas.SetActive(false);
        graphicsCanvas.SetActive(false);
        gammaCanvas.SetActive(false);

        switch (currentMenu)
        {
            case 0:
                audioCanvas.SetActive(true);
                break;
            case 1:
                graphicsCanvas.SetActive(true);
                break;
            case 2:
                gammaCanvas.SetActive(true);
                break;
        }
    }
}
