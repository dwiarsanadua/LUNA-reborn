--[[
  Quest 492: [Special][Quest] Bon Cavalier
  Level Required: 53
  NPC Start: 44, NPC Complete: 44
  Rewards: EXP=625097, Gold=174420
]]

local fsm = require('fsm_engine')

local quest_492 = fsm:new({
    id = 492,
    name = "[Special][Quest] Bon Cavalier",
    level_required = 53,
    npc_start = 44,
    npc_complete = 44,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 492: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 44,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 492: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 492: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 394,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 492: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 44,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 492: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 492: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 44,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 492: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 492: Rewards given")
                self:give_rewards({
                    exp = 625097,
                    gold = 174420,
                    items = {
                        { item_id = 333, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_492