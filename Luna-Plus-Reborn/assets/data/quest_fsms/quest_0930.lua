--[[
  Quest 930: Story of a Pirate
  Level Required: 102
  NPC Start: 536, NPC Complete: 536
  Prerequisites: [929]
  Rewards: EXP=11447014, Gold=81769
]]

local fsm = require('fsm_engine')

local quest_930 = fsm:new({
    id = 930,
    name = "Story of a Pirate",
    level_required = 102,
    npc_start = 536,
    npc_complete = 536,
    prerequisites = {929},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 930: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 536,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 930: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 930: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 295,
                    count = 60,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 930: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 296,
                    count = 60,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 930: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 536,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 930: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 930: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 536,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 930: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 930: Rewards given")
                self:give_rewards({
                    exp = 11447014,
                    gold = 81769,
                    items = {
                        { item_id = 21000271, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_930