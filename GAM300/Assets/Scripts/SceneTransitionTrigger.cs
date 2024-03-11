using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using BeanFactory;
public class SceneTransitionTrigger : Script
{
    public int sceneIndex = 0;
    public SpriteRenderer fadeBlackSR;

    public float fadeDuration = 2f;

    void Start()
    {
        if (fadeBlackSR == null)
        {
            Console.WriteLine("No fade black SR assigned to SceneTransition");
            return;
        }
        StartCoroutine(StartFadeIn());
    }

    public IEnumerator StartFadeIn()
    {
        float timer = 0f;
        while (timer < fadeDuration)
        {
            fadeBlackSR.alpha = Mathf.Lerp(1, 0, timer / fadeDuration);
            timer += Time.deltaTime;
            yield return null;
        }
        SetEnabled(fadeBlackSR,false);
    }

    public IEnumerator StartFadeOut()
    {
        SetEnabled(fadeBlackSR, true);
        float timer = 0f;
        while (timer < fadeDuration)
        {
            fadeBlackSR.alpha = Mathf.Lerp(0, 1, timer / fadeDuration);
            timer += Time.deltaTime;
            yield return null;
        }
        switch (sceneIndex)
        {
            case 0:
                SceneManager.LoadScene("LevelPlay2", true);
                break;
            case 1:
                SceneManager.LoadScene("LevelBoss", true);
                break;
            case 2:
                SceneManager.LoadScene("VictoryScreenMenu", true);
                break;
        }
    }

    void OnTriggerEnter(PhysicsComponent rb)
    {
        if (GetTag(rb) == "Player")
        {
            InternalCalls.StopMusic(fadeDuration*2f);
            StartCoroutine(StartFadeOut());
        }
    }
}