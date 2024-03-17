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


    //sounds
    public AudioSource bgm;


    void Start()
    {
        bgm.Play();
        //startGridTextSize = new vec3(mainMenuTitle.transform.localScale);
        //currentRestTimer = restTimer;
        StartCoroutine(ScrollCredits());
        //goToPlay();


    }

    //void goToPlay()
    //{
    //    if (Input.GetKeyDown(KeyCode.Space))
    //    {
    //        SceneManager.LoadScene("MainMenu", true);
    //    }
    //}

    IEnumerator ScrollCredits()
    {
        vec3 initialPos = credits.transform.position;
        //float distanceToMove = credits.GetComponent<SpriteRenderer>().
        //Transform creditsTransform = credits.GetComponent<Transform>();
        //vec3 translation = vec3.UnitY * scrollspeed * Time.deltaTime;

        //float distanceToMove = credits.GetComponent<SpriteRenderer>()
        while (true)
        {
            credits.transform.Translate(vec3.UnitY * scrollspeed * Time.deltaTime);

            yield return null;
        }

        //GoToMainMenu();


    }
    void GoToMainMenu()
    {
        SceneManager.LoadScene("MainMenu", true);
    }

    //void Update()
    //{
    //    //goToPlay();

    //}



}
