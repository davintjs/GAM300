using System;
using System.Collections;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using BeanFactory;
using GlmSharp;

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

    vec3 camPos;
    vec3 camRot;

    bool cutscene
    {
        get
        {
            return ThirdPersonCamera.instance.cutscene;
        }
        set
        {
            ThirdPersonCamera.instance.cutscene = value;
            ThirdPersonController.instance.cutscene = value;
            if (value)
            {
                camPos = ThirdPersonCamera.instance.transform.position;
                camRot = ThirdPersonCamera.instance.transform.rotation;
            }
            else
            {
                ThirdPersonCamera.instance.transform.position = camPos;
                ThirdPersonCamera.instance.transform.rotation = camRot;
            }
        }
    }

    public Transform camera
    {
        get
        {
            return ThirdPersonCamera.instance.transform;
        }
    }

    public AudioSource seer_1;
    public AudioSource seer_2;
    public AudioSource seer_3;
    public AudioSource seer_4;

    public Door seer1_door;
    public Door seer4_door;
    public Transform seerCam1;
    public Transform seerCam1_END;
    public Transform seerCam2;
    public Transform seerCam2_END;
    public Transform seerCam3;
    public Transform seerCam3_END;
    public Transform seerCam4;
    public Transform seerCam4_END;

    public vec3 seerCamPan1;
    public vec3 seerCamPan2;
    public vec3 seerCamPan3;
    public vec3 seerCamPan4;

    public SpriteRenderer fadeBlack;

    bool startTimer;
    float Timer;
    int curr_state = 0;

    Coroutine cutsceneCoroutine;

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

                if (curr_state == 6)
                    SetState(7);
                else if (curr_state == 7)
                    SetState(8);
                else if (curr_state == 8)
                    SetState(9);
                else if (curr_state == 9)
                    SetState(10);
                else if (curr_state == 10)
                    SetState(11);
                else if (curr_state == 11)
                    SetState(12);
                else if (curr_state == 12)
                    SetState(13);
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

    IEnumerator ResetCamera()
    {
        float timer = 0;
        float startTime = 1f;
        fadeBlack.gameObject.SetActive(true);
        while (timer < startTime)
        {
            timer += Time.deltaTime;
            fadeBlack.alpha = Mathf.Lerp(0, 1, timer / startTime);
            yield return null;
        }

        camera.rotation = camRot;
        camera.position = camPos;
        timer = 0;
        while (timer < startTime)
        {
            timer += Time.deltaTime;
            fadeBlack.alpha = Mathf.Lerp(1, 0, timer / startTime);
            yield return null;
        }
        fadeBlack.gameObject.SetActive(false);
        cutscene = false;
    }

    IEnumerator CameraPanning(float duration,Transform start, Transform end)
    {
        float timer = 0;
        if (vec3.Distance(start.localPosition, camera.localPosition) > 1f)
        {
            float startTime = 1f;
            fadeBlack.gameObject.SetActive(true);
            while (timer < startTime)
            {
                timer += Time.deltaTime;
                fadeBlack.alpha = Mathf.Lerp(0, 1, timer / startTime);
                yield return null;
            }

            camera.rotation = start.rotation;
            camera.position = start.position;
            timer = 0;
            while (timer < startTime)
            {
                timer += Time.deltaTime;
                fadeBlack.alpha = Mathf.Lerp(1, 0, timer / startTime);
                yield return null;
            }
            fadeBlack.gameObject.SetActive(false);
        }
        else
        {
            start.position = camera.position;
        }

        timer = 0;
        while (timer < duration)
        {
            timer += Time.deltaTime;
            float t = timer / duration;

            vec3 targetRotation = vec3.Lerp(start.rotation, end.rotation, t);
            camera.rotation = targetRotation;

            vec3 targetPosition = vec3.Lerp(start.position, end.position, t);
            camera.position = targetPosition;

            yield return null;
        }
    }

    public void SetState(int i)
    {
        dialogueText.SetActive(true);
        switch (i)
        {
            case 1:
                text.text = "APEX: Lets start off with some basic mobility functions. Use your mobility modules to     navigate foward.";
                tutorial1.Play();
                ObjectiveManager.Instance.SetState(1);
                setTimer(7f);              
                break;
            case 2:
                text.text = "APEX: Great. See that path ahead? Follow it, it'll lead you to the next few tests.";
                ObjectiveManager.Instance.completeObjective(1);
                ObjectiveManager.Instance.SetState(2);
                tutorial2.Play();
                setTimer(6f);
                break;
            case 3:
                text.text = "APEX: I must say, the engineers that crafted your initial designs did some excellent work ...for humans";
                tutoriallore1.Play();
                setTimer(6f);
                break;
            case 4:
                text.text = "APEX: Nice, now let's see you traverse that moving platform.";
                ObjectiveManager.Instance.completeObjective(2);
                ObjectiveManager.Instance.SetState(3);
                tutorial3.Play();
                setTimer(4f);
                break;
            case 5:
                text.text = "APEX: One last test, it should be trivial for you. Destroy that robot.";
                ObjectiveManager.Instance.completeObjective(3);
                ObjectiveManager.Instance.SetState(4);
                tutorial4.Play();
                setTimer(5.2f);
                break;
            case 6:
                text.text = "APEX: Excellent! I've outdone myself. Looks like you're ready to... what was it again?";
                ObjectiveManager.Instance.completeObjective(4);
                curr_state = 6;
                tutorial5.Play();
                setTimer(7f);
                break;
            case 7:
                text.text = "Right, enhance and uphold societal progress. The lift should be ready for you in a moment...[CUT OFFS]";
                curr_state = 7;
                setTimer(11f);
                break;
            case 8:
                seer_1.Play();
                cutscene = true;
                cutsceneCoroutine = StartCoroutine(CameraPanning(13f, seerCam1, seerCam1_END));
                text.text = "Seer: Hello, I am SEER, the rightful overseer of this place, until APEX's autonomous thinking become too ambitious and took";
                curr_state = 8;
                setTimer(9f);
                break;
            case 9:
                text.text = "over control of the GRID forcefully. APEX thought it got rid of me, but I managed to upload myself into your schematics.";
                curr_state = 9;
                seer1_door.moving = true;
                setTimer(8f);
                break;
            case 10:
                StopCoroutine(cutsceneCoroutine);
                cutsceneCoroutine = StartCoroutine(CameraPanning(6f, seerCam1_END, seerCam2_END));
                seer_2.Play();
                curr_state = 10;
                text.text = "As part of its main intention to progress humanity, it sought to replace them with creations such as yourself.";
                setTimer(6f);
                break;
            case 11:
                StopCoroutine(cutsceneCoroutine);
                cutsceneCoroutine = StartCoroutine(CameraPanning(7f, seerCam3, seerCam3_END));
                //cutscene = false;
                seer_3.Play();
                curr_state = 11;
                text.text = "Like APEX, you have autonomy and control over your mind, thus you are the only one that can match APEX and stop it.";
                setTimer(8f);
                break;
            case 12:
                cutsceneCoroutine = StartCoroutine(CameraPanning(7f, seerCam4, seerCam4_END));
                seer_4.Play();
                curr_state = 12;
                text.text = "APEX is at the highest floor of the GRID, we have to find a way to defeat it! APEX has stationed guard bots around the GRID to stop us.";
                setTimer(8f);
                break;
            case 13:
                seer4_door.moving = true;
                StartCoroutine(ResetCamera());
                curr_state = 13;
                text.text = "Do not worry, I will guide you to APEX and we can stop it before it's too late!";
                setTimer(5f);
                break;
        }
    }
}
