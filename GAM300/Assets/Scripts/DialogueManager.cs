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

    public GameObject dialogueText;
    TextRenderer text;
    //public GameObject text1;
    //public GameObject text2;
    //public GameObject text3;
    //public GameObject text4;
    //public GameObject text5;
    //public GameObject text6;
    //public GameObject text7;

    public AudioSource tutorial1;
    public AudioSource tutorial2;
    public AudioSource tutoriallore1;
    public AudioSource tutorial3;
    public AudioSource tutorial4;
    public AudioSource tutorial5;
    public AudioSource tutorial6;

    bool startTimer;
    float Timer;
    int curr_state = 0;

    void Awake()
    {
        Instance = this;
        startTimer = false;
        text = dialogueText.GetComponent<TextRenderer>();
        curr_state = 0;
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
                if(curr_state == 6)
                {
                    SetState(7);
                }
                else
                {
                    dialogueText.SetActive(false);
                }
            }
        }
    }

    public void Reset()
    {
        //text1.SetActive(false);
        //text2.SetActive(false);
        //text3.SetActive(false);
        //text4.SetActive(false);
        //text5.SetActive(false);
        //text6.SetActive(false);
        //text7.SetActive(false);
    }

    void setTimer (float duration)
    {
        startTimer = true;
        Timer = duration;
    }

    public void SetState(int i)
    {
        dialogueText.SetActive (true);
        switch (i)
        {
            case 1:
                //text1.SetActive(true);
                text.text = "Apex: Make use of your mobility modules to traverse those steps.";
                tutorial1.Play();
                ObjectiveManager.Instance.SetState(1);
                setTimer(7.5f);              
                break;
            case 2:
                //text2.SetActive(true);
                text.text = "Apex: Good. See that path ahead? Follow it, it'll lead you to the next few tests.";
                ObjectiveManager.Instance.completeObjective(1);
                ObjectiveManager.Instance.SetState(2);
                tutorial2.Play();
                setTimer(6f);
                break;
            case 3:
                //text3.SetActive(true);
                text.text = "Apex: I must say, the engineers that crafted your initial designs did some excellent work for humans";
                tutoriallore1.Play();
                setTimer(6f);
                break;
            case 4:
                //text4.SetActive(true);
                text.text = "Apex: Now let's see you traverse that moving platform.";
                ObjectiveManager.Instance.completeObjective(2);
                ObjectiveManager.Instance.SetState(3);
                tutorial3.Play();
                setTimer(4f);
                break;
            case 5:
                //text5.SetActive(true);
                text.text = "Apex: One last test, it should be trivial for you. Destroy that robot.";
                ObjectiveManager.Instance.completeObjective(3);
                ObjectiveManager.Instance.SetState(4);
                tutorial4.Play();
                setTimer(6f);
                break;
            case 6:
                text.text = "Apex: Excellent! I've outdone myself. Looks like you're ready to... what was it again?";
                ObjectiveManager.Instance.completeObjective(4);
                curr_state = 6;
                tutorial5.Play();
                setTimer(7f);
                break;
            case 7:
                //text7.SetActive(true);
                text.text = "Right… enhance and uphold societal progress. The lift should be ready for you in a moment…";
                curr_state = 7;
                tutorial6.Play();
                setTimer(7f);
                break;
        }
    }
}
