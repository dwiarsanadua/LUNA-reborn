--[[
  Quest 828: Devastated by the Surrounding
  Level Required: 77
  NPC Start: 93, NPC Complete: 93
  Rewards: EXP=1712009, Gold=41009
]]

local fsm = require('fsm_engine')

local quest_828 = fsm:new({
    id = 828,
    name = "Devastated by the Surrounding",
    level_required = 77,
    npc_start = 93,
    npc_complete = 93,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 828: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 93,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 828: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 828: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 250,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 828: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 93,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 828: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 828: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 93,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 828: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 828: Rewards given")
                self:give_rewards({
                    exp = 1712009,
                    gold = 41009,
                })
            end,
            transitions = {},
        },
    },
})

return quest_828