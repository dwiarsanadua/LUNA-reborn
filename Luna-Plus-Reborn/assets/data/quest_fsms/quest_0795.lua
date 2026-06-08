--[[
  Quest 795: Disgusting things
  Level Required: 64
  NPC Start: 91, NPC Complete: 91
  Rewards: EXP=567069, Gold=14506
]]

local fsm = require('fsm_engine')

local quest_795 = fsm:new({
    id = 795,
    name = "Disgusting things",
    level_required = 64,
    npc_start = 91,
    npc_complete = 91,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 795: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 91,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 795: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 795: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 249,
                    count = 15,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 795: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 91,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 795: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 795: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 91,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 795: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 795: Rewards given")
                self:give_rewards({
                    exp = 567069,
                    gold = 14506,
                    items = {
                        { item_id = 21000009, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_795