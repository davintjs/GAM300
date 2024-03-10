using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using BeanFactory;
class SceneTransitionTrigger : Script
{
    public int sceneIndex = 0;
    public SpriteRenderer fadeBlackSR;

    public float fadeDuration = 2f;

    float timer = 0f;

    bool fadeOut = false;

    void Update()
    {
        if (fadeBlackSR == null) 
        {
            Console.WriteLine("No fade black SR assigned to SceneTransition");
            return;
        }

        if (timer < fadeDuration) 
        {
            timer += Time.deltaTime;
        }

        if (fadeOut)
        {
            fadeBlackSR.alpha = Mathf.Lerp(0, 1, timer / fadeDuration);
            if (timer >= fadeDuration)
            {
                switch (sceneIndex)
                {
                    case 0:
                        SceneManager.LoadScene("LevelPlay2");
                        break;
                    case 1:
                        SceneManager.LoadScene("LevelBoss");
                        break;
                    case 2:
                        SceneManager.LoadScene("VictoryScreen", true);
                        break;
                }
            }
        }
        else
        {
            fadeBlackSR.alpha = Mathf.Lerp(1, 0, timer / fadeDuration);
        }
    }

    void OnTriggerEnter(PhysicsComponent rb)
    {
        if (GetTag(rb) == "Player")
        {
            fadeOut = true;
            timer = 0;
            InternalCalls.StopMusic(fadeDuration*2f);
        }
    }
}