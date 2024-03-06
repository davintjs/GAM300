using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using BeanFactory;

public class DialogueManager : Script 
{
    public static DialogueManager Instance;

    public GameObject text1;
    public GameObject text2;
    public GameObject text3;
    public GameObject text4;
    public GameObject text5;
    public GameObject text6;
    public GameObject text7;

    public AudioSource tutorial1;
    public AudioSource tutorial2;
    public AudioSource tutoriallore1;
    public AudioSource tutorial3;
    public AudioSource tutorial4;
    public AudioSource tutorial5;
    public AudioSource tutorial6;

    bool startTimer;
    float Timer;

    void Awake()
    {
        Instance = this;
    }

    void Update()
    {
        if (startTimer)
        {
            if(Timer > 0)
            {
                Timer -= Time.deltaTime;
            }
            else
            {
                startTimer = false;
                Reset();
            }
        }
    }

    public void Reset()
    {
        text1.SetActive(false);
        text2.SetActive(false);
        text3.SetActive(false);
        text4.SetActive(false);
        text5.SetActive(false);
        text6.SetActive(false);
        text7.SetActive(false);
    }

    void setTimer (float duration)
    {
        startTimer = true;
        Timer = duration;
    }

    public void SetState(int i)
    {
        switch (i)
        {
            case 1:
                text1.SetActive(true);
                tutorial1.Play();
                setTimer(7.5f);
                break;
            case 2:
                text2.SetActive(true);
                tutorial2.Play();
                setTimer(6f);
                break;
            case 3:
                text3.SetActive(true);
                tutoriallore1.Play();
                setTimer(6f);
                break;
            case 4:
                text4.SetActive(true);
                tutorial3.Play();
                setTimer(4f);
                break;
            case 5:
                text5.SetActive(true);
                tutorial4.Play();
                setTimer(6f);
                break;
            case 6:
                text6.SetActive(true);
                tutorial5.Play();
                setTimer(17f);
                break;
            case 7:
                text7.SetActive(true);
                tutorial6.Play();
                break;
        }
    }
}
