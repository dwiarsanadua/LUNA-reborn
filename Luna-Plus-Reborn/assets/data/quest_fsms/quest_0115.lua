--[[
  Quest 115: Evil Golem
  Level Required: 25
  NPC Start: 57, NPC Complete: 61
  Prerequisites: [105]
  Rewards: EXP=19326, Gold=3475
]]

local fsm = require('fsm_engine')

local quest_115 = fsm:new({
    id = 115,
    name = "Evil Golem",
    level_required = 25,
    npc_start = 57,
    npc_complete = 61,
    prerequisites = {105},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 115: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 61,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 115: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 115: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 82,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 115: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 61,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 115: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 57,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 115: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 115: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 61,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 115: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 115: Rewards given")
                self:give_rewards({
                    exp = 19326,
                    gold = 3475,
                    items = {
                        { item_id = 54, count = 1 },
                        { item_id = 30000824, count = 1 },
                        { item_id = 21000008, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_115