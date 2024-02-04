using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BeanFactory;

public class SceneLoadManager : Script
{
    public float loadDuration = 4f;

    public float fadeDuration = 5f;

    float timer = 0f;

    public FadeEffect fadeToBlack;

    void Awake()
    {
        fadeToBlack.inDuration = fadeDuration;
        fadeToBlack.outDuration = fadeDuration;
    }

    void Update()
    {
        if (timer < loadDuration)
        {
            timer += Time.deltaTime;
            if (timer >= loadDuration)
            {
                fadeToBlack.inAndOut = true;
            }
        }
        else if (timer < loadDuration + fadeDuration)
        {
            timer += Time.deltaTime;
        }
        else
        {
            SceneManager.LoadNext();
        }
    }
}
