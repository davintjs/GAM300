using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BeanFactory;
using GlmSharp;

public class Credits : Script
{
    public GameObject credits;

    public float scrollspeed = 5.0f;

    public float yOffset = 10f;

    vec3 startPos;

    float timer = 0f;

    public float scrollDuration = 10f;
    
    //sounds
    public AudioSource bgm;

    public FadeEffect fader;

    private bool waitingOnFader = false;


    void Start()
    {
        bgm.Play();
        //startGridTextSize = new vec3(mainMenuTitle.transform.localScale);
        //currentRestTimer = restTimer;
        //goToPlay();


        startPos = credits.transform.localPosition;
        fader.StartFadeOut(2f, false, 0.5f, 0f);
    }

    void Update()
    {

        if (timer < scrollDuration)
        {
            credits.transform.localPosition = vec3.Lerp(startPos, startPos + vec3.UnitY * yOffset, timer / scrollDuration);
            timer += Time.unscaledDeltaTime;
            if (timer >= scrollDuration)
            {
                fader.StartFadeIn(1f, true, 2f, 0.5f);
                waitingOnFader = true;
            }
        }

        if(Input.GetKeyDown(KeyCode.Escape) && !waitingOnFader)
        {
            fader.StartFadeIn(1f, true, 2f, 0.5f);
            waitingOnFader = true;
        }

        if(waitingOnFader && fader.finished)
        {
            GoToMainMenu();
        }

    }

    void GoToMainMenu()
    {
        SceneManager.LoadScene("MainMenu", true);
    }
}
