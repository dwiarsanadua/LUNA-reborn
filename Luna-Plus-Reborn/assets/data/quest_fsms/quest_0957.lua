--[[
  Quest 957: New Monsters 2
  Level Required: 109
  NPC Start: 128, NPC Complete: 128
  Prerequisites: [955]
  Rewards: EXP=20639339, Gold=101632
]]

local fsm = require('fsm_engine')

local quest_957 = fsm:new({
    id = 957,
    name = "New Monsters 2",
    level_required = 109,
    npc_start = 128,
    npc_complete = 128,
    prerequisites = {955},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 957: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 128,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 957: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 957: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 345,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 957: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 128,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 957: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 957: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 128,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 957: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 957: Rewards given")
                self:give_rewards({
                    exp = 20639339,
                    gold = 101632,
                    items = {
                        { item_id = 414, count = 1 },
                        { item_id = 414, count = 30 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_957