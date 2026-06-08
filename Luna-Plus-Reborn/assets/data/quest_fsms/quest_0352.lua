--[[
  Quest 352: [Party] Subterranean Rescue
  Level Required: 33
  NPC Start: 59, NPC Complete: 86
  Prerequisites: [317]
  Rewards: EXP=44517, Gold=3000
]]

local fsm = require('fsm_engine')

local quest_352 = fsm:new({
    id = 352,
    name = "[Party] Subterranean Rescue",
    level_required = 33,
    npc_start = 59,
    npc_complete = 86,
    prerequisites = {317},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 352: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 59,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 352: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 352: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 59,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 352: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 84,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 352: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 85,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 352: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 86,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 352: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 352: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 86,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 352: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 352: Rewards given")
                self:give_rewards({
                    exp = 44517,
                    gold = 3000,
                    items = {
                        { item_id = 258, count = 1 },
                        { item_id = 259, count = 1 },
                        { item_id = 260, count = 1 },
                        { item_id = 21000282, count = 2 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_352