--[[
  Quest 184: Black Magic
  Level Required: 41
  NPC Start: 10, NPC Complete: 34
  Prerequisites: [183]
  Rewards: EXP=110196, Gold=6105
]]

local fsm = require('fsm_engine')

local quest_184 = fsm:new({
    id = 184,
    name = "Black Magic",
    level_required = 41,
    npc_start = 10,
    npc_complete = 34,
    prerequisites = {183},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 184: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 34,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 184: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 184: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 44,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 184: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 34,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 184: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 184: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 184: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 34,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 184: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 184: Rewards given")
                self:give_rewards({
                    exp = 110196,
                    gold = 6105,
                    items = {
                        { item_id = 93, count = 10 },
                        { item_id = 30000044, count = 7 },
                        { item_id = 21000097, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_184