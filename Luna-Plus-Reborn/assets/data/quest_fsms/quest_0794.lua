--[[
  Quest 794: I just hoped
  Level Required: 63
  NPC Start: 44, NPC Complete: 44
  Rewards: EXP=519714, Gold=14506
]]

local fsm = require('fsm_engine')

local quest_794 = fsm:new({
    id = 794,
    name = "I just hoped",
    level_required = 63,
    npc_start = 44,
    npc_complete = 44,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 794: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 44,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 794: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 794: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 66,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 794: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 44,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 794: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 794: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 44,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 794: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 794: Rewards given")
                self:give_rewards({
                    exp = 519714,
                    gold = 14506,
                    items = {
                        { item_id = 21000282, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_794