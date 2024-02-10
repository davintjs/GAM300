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

    public FadeEffect digipenFade;
    public FadeEffect gameLogoFade;

    public TransformEffect gameLogoTransform;

    int state = 0;

    void Start()
    {
        digipenFade.StartFadeInAndOut();
    }

    void Update()
    {
        if (digipenLogo == null || gameLogo == null)
            return;
        if (digipenFade == null || gameLogoFade == null)
            return;
        if (gameLogoTransform == null)
            return;

        switch (state) 
        {
            case 0:
                if (digipenFade.finished || Input.GetMouseDown(0))
                {
                    ++state;
                    //gameLogoFade.Start();
                    digipenFade.gameObject.SetActive(false);
                    gameLogoFade.StartFadeInAndOut();
                }
                break;
            case 1:
                if (gameLogoFade.finished || Input.GetMouseDown(0))
                {
                    ++state;
                    
                    SceneManager.LoadScene("MainMenu", true);
                }
                break;
        }      
    }
}
