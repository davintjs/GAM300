using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BeanFactory;

public class Splashscreen : Script
{
    public FadeEffect digipenFade;
    public FadeEffect gameLogoFade;
    public FadeEffect bgFade;

    public TransformEffect gameLogoTransform;

    int state = 0;

    void Start()
    {
        if (digipenFade == null || gameLogoFade == null || bgFade == null)
            return;
        if (gameLogoTransform == null)
            return;

        digipenFade.StartFadeInAndOut();
    }

    void Update()
    {
        if (digipenFade == null || gameLogoFade == null || bgFade == null)
            return;
        if (gameLogoTransform == null)
            return;

        return;
        switch (state) 
        {
            case 0:
                if (digipenFade.finished || Input.GetMouseDown(0))
                {
                    ++state;
                    digipenFade.fader.SetActive(false);

                    if(!gameLogoFade.IsFading())
                        gameLogoFade.StartFadeInAndOut();
                }
                break;
            case 1:
                if (gameLogoFade.finished || Input.GetMouseDown(0))
                {
                    ++state;
                    gameLogoFade.fader.SetActive(false);

                    if (!bgFade.IsFading())
                        bgFade.StartFadeInAndOut();
                }
                break;
            case 2:
                if (bgFade.finished || Input.GetMouseDown(0))
                {
                    bgFade.fader.SetActive(false);
                }
                break;
        }      
    }

    void SetState(int state_)
    {

    }

}
