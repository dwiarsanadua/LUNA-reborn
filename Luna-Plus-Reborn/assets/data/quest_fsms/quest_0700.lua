--[[
  Quest 700: A favor for Kasya
  Level Required: 48
  NPC Start: 41, NPC Complete: 41
  Rewards: EXP=149684
]]

local fsm = require('fsm_engine')

local quest_700 = fsm:new({
    id = 700,
    name = "A favor for Kasya",
    level_required = 48,
    npc_start = 41,
    npc_complete = 41,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 700: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 41,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 700: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 700: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 13,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 700: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 41,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 700: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 700: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 41,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 700: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 700: Rewards given")
                self:give_rewards({
                    exp = 149684,
                })
            end,
            transitions = {},
        },
    },
})

return quest_700