--[[
  Quest 77: Delivering The Armor
  Level Required: 4
  NPC Start: 6, NPC Complete: 11
  Prerequisites: [72]
  Rewards: EXP=50, Gold=750
]]

local fsm = require('fsm_engine')

local quest_77 = fsm:new({
    id = 77,
    name = "Delivering The Armor",
    level_required = 4,
    npc_start = 6,
    npc_complete = 11,
    prerequisites = {72},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 77: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 6,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 77: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 77: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 6,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 77: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 11,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 77: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 77: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 11,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 77: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 77: Rewards given")
                self:give_rewards({
                    exp = 50,
                    gold = 750,
                    items = {
                        { item_id = 21000006, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_77