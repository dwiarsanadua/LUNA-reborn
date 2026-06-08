--[[
  Quest 19: Dethroning the Goblin King
  Level Required: 17
  NPC Start: 22, NPC Complete: 22
  Prerequisites: [13]
  Rewards: EXP=4161, Gold=3475
]]

local fsm = require('fsm_engine')

local quest_19 = fsm:new({
    id = 19,
    name = "Dethroning the Goblin King",
    level_required = 17,
    npc_start = 22,
    npc_complete = 22,
    prerequisites = {13},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 19: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 22,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 19: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 19: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 34,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 19: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 22,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 19: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 19: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 22,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 19: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 19: Rewards given")
                self:give_rewards({
                    exp = 4161,
                    gold = 3475,
                    items = {
                        { item_id = 21000008, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_19