--[[
  Quest 604: Tower of Evil (Advanced)
  Level Required: 50
  NPC Start: 28, NPC Complete: 29
  Rewards: EXP=190259
]]

local fsm = require('fsm_engine')

local quest_604 = fsm:new({
    id = 604,
    name = "Tower of Evil (Advanced)",
    level_required = 50,
    npc_start = 28,
    npc_complete = 29,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 604: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 28,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 604: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 604: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 495,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 604: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 28,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 604: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 29,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 604: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 604: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 29,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 604: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 604: Rewards given")
                self:give_rewards({
                    exp = 190259,
                })
            end,
            transitions = {},
        },
    },
})

return quest_604