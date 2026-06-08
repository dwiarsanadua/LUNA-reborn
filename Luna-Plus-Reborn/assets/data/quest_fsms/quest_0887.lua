--[[
  Quest 887: Creatures in Wonderland
  Level Required: 96
  NPC Start: 540, NPC Complete: 540
  Rewards: EXP=7628195, Gold=77721
]]

local fsm = require('fsm_engine')

local quest_887 = fsm:new({
    id = 887,
    name = "Creatures in Wonderland",
    level_required = 96,
    npc_start = 540,
    npc_complete = 540,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 887: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 540,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 887: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 887: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 321,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 887: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 322,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 887: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 540,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 887: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 887: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 540,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 887: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 887: Rewards given")
                self:give_rewards({
                    exp = 7628195,
                    gold = 77721,
                })
            end,
            transitions = {},
        },
    },
})

return quest_887