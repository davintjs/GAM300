using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BeanFactory;

public class Splashscreen : Script
{
    public SpriteRenderer digipenLogo;
    public SpriteRenderer gameLogo;
    public SpriteRenderer bg;

    public FadeEffect digipenFade;
    public FadeEffect gameLogoFade;
    public FadeEffect bgFade;

    public TransformEffect gameLogoTransform;

    int state = 0;

    void Update()
    {
        if (digipenLogo == null || gameLogo == null || bg == null)
            return;
        if (digipenFade == null || gameLogoFade == null || bgFade == null)
            return;
        if (gameLogoTransform == null)
            return;

        switch (state) 
        {
            case 0:
                if (digipenFade.finished || Input.GetMouseDown(0))
                {
                    ++state;
                    gameLogoFade.Start();
                    gameLogoFade.gameObject.SetActive(true);
                    digipenFade.gameObject.SetActive(false);
                }
                break;
            case 1:
                if (gameLogoFade.finished || Input.GetMouseDown(0))
                {
                    ++state;
                    bgFade.start = true;
                    gameLogoFade.gameObject.SetActive(false);
                }
                break;
            case 2:
                if (bgFade.finished || Input.GetMouseDown(0))
                {
                    bgFade.gameObject.SetActive(false);
                    SceneManager.LoadScene("MainMenu", true);
                }
                break;
        }      
    }

    void SetState(int state_)
    {

    }

}
