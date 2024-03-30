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
    public FadeEffect fader;
    public bool triggerEnter = false;

    void Awake()
    {
        if(fader == null)
        {
            Console.WriteLine("Please assign FadeEffect script in SceneTransitionTrigger!");
        }
    }

    void Start()
    {
        fader.StartFadeOut();
    }

    void Update()
    {
        if(triggerEnter && fader.finished)
        {
            switch (sceneIndex)
            {
                case 0:
                    SceneManager.LoadScene("LevelPlay2", true);
                    break;
                case 1:
                    SceneManager.LoadScene("LevelBoss", true);
                    break;
                case 2:
                    SceneManager.LoadScene("Credits", true);
                    break;
            }
        }
    }

    void OnTriggerEnter(PhysicsComponent rb)
    {
        if (GetTag(rb) == "Player")
        {
            InternalCalls.StopMusic(4f);
            fader.StartFadeIn(1f, true, 0f, 0.5f);
            triggerEnter = true;
        }
    }
}