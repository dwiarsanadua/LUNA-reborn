--[[
  Quest 922: [Party] I Hate Mosquitos!
  Level Required: 100
  NPC Start: 541, NPC Complete: 541
  Prerequisites: [896]
  Rewards: EXP=11363827, Gold=81769
]]

local fsm = require('fsm_engine')

local quest_922 = fsm:new({
    id = 922,
    name = "[Party] I Hate Mosquitos!",
    level_required = 100,
    npc_start = 541,
    npc_complete = 541,
    prerequisites = {896},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 922: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 541,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 922: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 922: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 320,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 922: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 541,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 922: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 922: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 541,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 922: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 922: Rewards given")
                self:give_rewards({
                    exp = 11363827,
                    gold = 81769,
                    items = {
                        { item_id = 21000251, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_922