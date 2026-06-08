--[[
  Quest 818: Magical Creatures
  Level Required: 73
  NPC Start: 92, NPC Complete: 92
  Rewards: EXP=1482977, Gold=21542
]]

local fsm = require('fsm_engine')

local quest_818 = fsm:new({
    id = 818,
    name = "Magical Creatures",
    level_required = 73,
    npc_start = 92,
    npc_complete = 92,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 818: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 92,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 818: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 818: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 259,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 818: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 92,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 818: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 818: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 92,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 818: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 818: Rewards given")
                self:give_rewards({
                    exp = 1482977,
                    gold = 21542,
                })
            end,
            transitions = {},
        },
    },
})

return quest_818