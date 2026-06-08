--[[
  Quest 920: Necklace of Blue Beard Pirate
  Level Required: 0
  NPC Start: 537, NPC Complete: 537
  Prerequisites: [919]
  Rewards: EXP=14354308, Gold=81769
]]

local fsm = require('fsm_engine')

local quest_920 = fsm:new({
    id = 920,
    name = "Necklace of Blue Beard Pirate",
    level_required = 0,
    npc_start = 537,
    npc_complete = 537,
    prerequisites = {919},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 920: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 537,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 920: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 920: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 295,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 920: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 294,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 920: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 537,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 920: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 920: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 537,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 920: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 920: Rewards given")
                self:give_rewards({
                    exp = 14354308,
                    gold = 81769,
                    items = {
                        { item_id = 405, count = 1 },
                        { item_id = 405, count = 1 },
                        { item_id = 405, count = 50 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_920