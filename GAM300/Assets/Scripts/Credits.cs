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


    void Start()
    {
        bgm.Play();
        //startGridTextSize = new vec3(mainMenuTitle.transform.localScale);
        //currentRestTimer = restTimer;
        //goToPlay();


        startPos = credits.transform.localPosition;
    }

    void Update()
    {

        if (timer < scrollDuration)
        {
            credits.transform.localPosition = vec3.Lerp(startPos, startPos + vec3.UnitY * yOffset, timer / scrollDuration);
            timer += Time.unscaledDeltaTime;
            if (timer >= scrollDuration)
            {
                StartCoroutine(WaitTime());
            }
        }

    }

    IEnumerator WaitTime()
    {
        yield return new WaitForSeconds(3f);
        GoToMainMenu();
    }

    //void goToPlay()
    //{
    //    if (Input.GetKeyDown(KeyCode.Space))
    //    {
    //        SceneManager.LoadScene("MainMenu", true);
    //    }
    //}
    void GoToMainMenu()
    {
        SceneManager.LoadScene("VictoryScreenMenu", true);
    }

    //void Update()
    //{
    //    //goToPlay();

    //}



}
