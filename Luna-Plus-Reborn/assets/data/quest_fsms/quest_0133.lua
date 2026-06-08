--[[
  Quest 133: Hunting Goblin Guards
  Level Required: 17
  NPC Start: 20, NPC Complete: 20
  Prerequisites: [101]
  Rewards: EXP=4161, Gold=1260
]]

local fsm = require('fsm_engine')

local quest_133 = fsm:new({
    id = 133,
    name = "Hunting Goblin Guards",
    level_required = 17,
    npc_start = 20,
    npc_complete = 20,
    prerequisites = {101},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 133: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 20,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 133: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 133: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 33,
                    count = 15,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 133: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 20,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 133: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 133: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 20,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 133: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 133: Rewards given")
                self:give_rewards({
                    exp = 4161,
                    gold = 1260,
                    items = {
                        { item_id = 21000006, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_133