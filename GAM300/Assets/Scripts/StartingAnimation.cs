using System;
using System.Collections;
using System.Collections.Generic;
using System.Dynamic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using BeanFactory;
using GlmSharp;

public class StartingAnimation : Script
{
    public GameObject outline;
    public GameObject systembooting;
    public GameObject systeminit;
    public GameObject loading;
    public GameObject stats;
    public GameObject statsok;
    public GameObject gps;
    public GameObject map;
    public GameObject location;
    public GameObject transmission;
    public GameObject scanning;
    public GameObject view;
    public GameObject visorconnected;

    public GameObject fadeblack;

    public GameObject text1;
    public GameObject text2;
    public GameObject text3;
    public GameObject text4;
    public GameObject text5;
    public GameObject text6;
    public GameObject text7;

    public AudioSource audio;

    public AudioSource sfx1;
    public AudioSource sfx2;
    public AudioSource sfx3;

    private float timer;
    private float textTimer;

    bool fadeout = false;
    bool playAudio = true;
    bool startText = false;

    bool seq1, seq2, seq3, seq4, seq5, seq6;

    void Awake()
    {
        timer = 0;
        systembooting.SetActive(true);
        outline.SetActive(true);
        startText = false;
        playAudio = true;
        textTimer = 0f;
        seq1 = true;
        seq2 = true;
        seq3 = true;
        seq4 = true;
        seq5 = true;
        seq6 = true;
        stop = false;
    }

    void AudioPlay()
    {
        playAudio = false;
        audio.Play();
    }

    bool stop = false;


    void Update()
    {
        Console.WriteLine(timer);
        if (Input.GetKeyDown(KeyCode.Escape)) {
            //skip to level
            timer = 65f;
        }
        timer += Time.deltaTime;

        if (timer < 10f)
        {
            SpriteRenderer black = fadeblack.GetComponent<SpriteRenderer>();
            if(black.alpha > 0)
            {
                black.alpha -= Time.deltaTime;
                if (black.alpha < 0)
                    fadeblack.SetActive(false);
            }

            SpriteRenderer systemboot = systembooting.GetComponent<SpriteRenderer>();
            if (systemboot.alpha > 0f && fadeout)
                systemboot.alpha -= Time.deltaTime / 2f;
            else
            {
                fadeout = false;
                systemboot.alpha += Time.deltaTime / 2f;
                if (systemboot.alpha >= 1f)
                {
                    fadeout = true;
                }
            }

            
        }
        else if (timer < 14f)
        {
            systembooting.SetActive(false);

            if (timer >= 11f)
            {
                if (seq4)
                {
                    seq4 = false;
                    transmission.SetActive(true);
                    sfx3.Play();
                }
            }


            if (timer >= 12f)
            {
                if (playAudio)
                {
                    AudioPlay();
                    startText = true;
                }
            }

        }
        else if (timer < 35f)
        {
            if (seq1)
            {
                seq1 = false;
                systeminit.SetActive(true);
                sfx1.Play();
            }

            if (timer > 17f)
                loading.SetActive(true);

            vec3 rot = loading.transform.rotation;
            rot.z += Time.deltaTime * 4f;
            loading.transform.rotation = rot;

            if (timer >= 20f)
                stats.SetActive(true);

            if (timer >= 22f)
            {
                if (seq2)
                {
                    seq2 = false;
                    sfx2.Play();
                    statsok.SetActive(true);
                }

            }

            if (timer >= 30f)
            {
                if (seq3)
                {
                    seq3 = false;
                    visorconnected.SetActive(true);
                    sfx2.Play();
                }
            }

            if (timer >= 32)
            {
                view.SetActive(true);
            }

        }
        else if (timer < 64f)
        {
            systeminit.SetActive(false);
            loading.SetActive(false);
            visorconnected.SetActive(false);
            scanning.SetActive(true);

            SpriteRenderer scan = scanning.GetComponent<SpriteRenderer>();

            if (!stop)
            {
                if (scan.alpha > 0f && fadeout)
                    scan.alpha -= Time.deltaTime / 2f;
                else
                {
                    fadeout = false;
                    scan.alpha += Time.deltaTime / 2f;
                    if (scan.alpha >= 1f)
                    {
                        sfx1.Play();
                        fadeout = true;
                    }
                }
            }

            if (timer > 50f && scan.alpha <= 0)
            {
                stop = true;
            }


            if (timer >= 44f)
            {
                if (seq5)
                {
                    seq5 = false;
                    gps.SetActive(true);
                    sfx1.Play();
                }

            }

            if (timer >= 47f)
            {
                if (seq6)
                {
                    seq6 = false;
                    map.SetActive(true);
                    location.SetActive(true);
                    sfx1.Play();
                }

            }

            
        }
        else if (timer < 75f)
        {

        }
        else
        {
            if (fadeblack.activeSelf == false)
                fadeblack.SetActive(true);
            SpriteRenderer black = fadeblack.GetComponent<SpriteRenderer>();
            if (black.alpha < 1f)
            {
                black.alpha += Time.deltaTime;
                if (black.alpha >= 1f)
                    SceneManager.LoadScene("LevelTutorial", true);

            }
        }

        //timers for subtitles
        if(startText)
        {
            textTimer += Time.deltaTime;

            if(textTimer < 14f)
            {
                text1.SetActive(true);
            }
            else if(textTimer < 19f)
            {
                text1.SetActive(false);
                text2.SetActive(true);
            }
            else if(textTimer < 34f)
            {
                text2.SetActive(false);
                text3.SetActive(true);            
            }
            else if (textTimer < 39f)
            {
                scanning.SetActive(false);
                text3.SetActive(false);
                text4.SetActive(true);
            }
            else if (textTimer < 48f)
            {
                text4.SetActive(false);
                text5.SetActive(true);
            }
            else if (textTimer < 56f)
            {
                text5.SetActive(false);
                text6.SetActive(true);
            }
            else if (textTimer < 63f)
            {
                text6.SetActive(false);
                text7.SetActive(true);
            }
        }
    }

}
