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
    }

    void AudioPlay()
    {
        playAudio = false;
        audio.Play();
    }



    void Update()
    {
        timer += Time.deltaTime;

        if (timer < 10f)
        {
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
        else if (timer < 11f)
        {
            systembooting.SetActive(false);
        }
        else if (timer < 25f)
        {
            if (seq1)
            {
                seq1 = false;
                systeminit.SetActive(true);
                sfx1.Play();
            }

            if (timer > 12f)
                loading.SetActive(true);

            vec3 rot = loading.transform.rotation;
            rot.z += Time.deltaTime * 4f;
            loading.transform.rotation = rot;

            if (timer >= 14f)
                stats.SetActive(true);

            if (timer >= 16f)
            {
                if (seq2)
                {
                    seq2 = false;
                    sfx2.Play();
                    statsok.SetActive(true);
                }
                
            }

            if (timer >= 17.5f)
            {
                if (seq3)
                {
                    seq3 = false;
                    transmission.SetActive(true);
                    sfx3.Play();
                }            
            }


            if (timer >= 18.5f)
            {
                if (playAudio)
                {
                    AudioPlay();
                    startText = true;
                }              
            }
        }
        else if (timer < 40f)
        {
            systeminit.SetActive(false);
            loading.SetActive(false);

            scanning.SetActive(true);

            if (timer >= 28f)
            {
                if (seq4)
                {
                   seq4 = false;
                    gps.SetActive(true);
                    sfx1.Play();
                }
                
            }
            if (timer >= 30f)
            {
                if (seq5)
                {
                    seq5 = false;
                    map.SetActive(true);
                    location.SetActive(true);
                    sfx1.Play();
                }

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
